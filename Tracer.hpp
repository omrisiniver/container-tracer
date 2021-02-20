#ifndef _TRACER_
#define _TRACER_

#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <experimental/filesystem>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <syscall.h>
#include <sys/user.h>
#include <algorithm>
#include <map>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

struct ProcMetaData
{
	std::vector<int> syscall;
	bool firstEntry = true;
	uint64_t timestamp = 0;

	ProcMetaData& operator=(const ProcMetaData& other)
	{
		if (this == &other)
		{
			return *this;
		}

		this->syscall = other.syscall;
		this->firstEntry = other.firstEntry;
		this->timestamp = other.timestamp;
		return *this;
	}
};

namespace fs = std::experimental::filesystem;

template <typename Product>
class tracer
{
public:
	tracer(dispatch_queue& queue)
		: m_queue(queue), m_procNames()
	{}

	bool Init(std::string&& configPath)
	{
		FILE* fd = fopen(configPath.data(), "r");

		if (fd == nullptr)
		{
			std::cout << "Failed open config file " << configPath.data() << std::endl;
			sleep(5);
			Init(std::move(configPath));
			// return false;
		}

		if (!ReadConfig(fd))
		{
			std::cout << "Failed read config file" << std::endl;
			return false;	
		}

		if (!ParseProcs())
		{
			std::cout << "Failed parse procs " << std::endl;
			return false;
		}

		std::cout << "Success " << m_procNames.size() << std::endl;

		return true;
	}

	void trace(void)
	{
		for (auto& [pid, value] : m_pidData)
		{
			if (ptrace(PTRACE_ATTACH, pid, nullptr) == -1)
			{
				std::cout << "Failed to trace " << pid << errno << std::endl;				
				return;
			}
		}

		int status = 1;
		pid_t currPid = 0;
		struct user_regs_struct regs;

		while(true)
		{
			ptrace(PTRACE_SYSCALL, currPid, nullptr, nullptr);
			currPid = waitpid(-1, &status, 0);

			int res = ptrace(PTRACE_GETREGS, currPid, 0, &regs);
			if (res < 0)
			{
				if (currPid < 0)
				{
					continue;
				} 
				m_currPid = currPid;
				m_currStatus = status;
				m_currRegs = regs;
				m_queue.dispatch([this]{handle_job();});
				continue;
			}

			m_currPid = currPid;
			m_currStatus = status;
			m_currRegs = regs;

			m_queue.dispatch([this]{handle_job();});
		}
	}

	void handle_job()
	{
		if (WIFEXITED(m_currStatus))
		{
			std::cout << "Syscall amount " << m_pidData[m_currPid].syscall.size() << std::endl;
			for(auto p: m_pidData[m_currPid].syscall)
			{
				std::cout << " " << p << " ";
			}
			m_pidData.erase(m_currPid);
			if (m_pidData.empty())
			{
				std::cout << std::endl << "All procs were killed" << std::endl;
				exit(0);
			}
		}

		if (m_pidData[m_currPid].firstEntry)
		{
			m_pidData[m_currPid].firstEntry = false;
			return ;
		}

		m_pidData[m_currPid].syscall.push_back(m_currRegs.orig_rax);
		m_pidData[m_currPid].timestamp = time(nullptr);
		m_pidData[m_currPid].firstEntry = true;
	}

	~tracer() = default;

private:

	bool ReadConfig(FILE* fd)
	{
		if (!fd)
		{
			return false;
		}

		size_t counter = 0;
		int currChar = 0;
		std::string procName;

		while ((currChar = getc(fd)) && currChar != EOF)
		{
			if (currChar == '\n')
			{
				m_procNames.push_back(procName);
				procName.clear();
				++counter;
			}

			procName.push_back(currChar);
		}

		return true;
	}

	bool ParseProcs()
	{
		for (auto& p: fs::directory_iterator("/proc"))
		{
			std::string currPath = p.path().string();
			size_t lastIndex = currPath.find_last_of('/');
			std::string currString = p.path().string().substr(lastIndex + 1, currPath.size());

			char* path;
			long converted = strtol(currString.c_str(), &path, 10);

			if (*path)
			{
				continue;
			}

			char buff[100] = {0};
			snprintf(buff, sizeof(buff), "/proc/%s/exe", currString.c_str());

			if (!FindBinName(std::move(std::string(buff))))
			{
				continue;
			}

			m_pidData[static_cast<int>(converted)] = *(new ProcMetaData());

			std::cout << currString.data() << "   " << static_cast<int>(converted) << std::endl;
		}

		if (m_pidData.size() != m_procNames.size())
		{
			std::cout << "pid " << m_pidData.size() << std::endl;
			std::cout << "names " << m_procNames.size() << std::endl;

			m_pidData.clear();
			sleep(5);
			ParseProcs();
		}

		return true;
	}

	bool FindBinName(std::string&& binName)
	{
		char path[100] = {0};
		ssize_t bytesRead = readlink(binName.c_str(), path, sizeof(path));

		if (bytesRead <= 0)
		{
			return false;
		}

		path[bytesRead] = '\0';
		const char * const name = strrchr(path, '/') + 1;  
		std::string s(name);

		for (auto& p: m_procNames)
		{
			if (p == s)
			{
				return true;
			}
		}

		return false;
	}
	
	pid_t m_currPid = 0;
	int m_currStatus = 0;
	struct user_regs_struct m_currRegs;
	std::queue<Product> m_queue_val;
	dispatch_queue& m_queue;
	std::vector<std::string> m_procNames;
	std::unordered_map<pid_t, ProcMetaData> m_pidData;
};


#endif // _TRACER_