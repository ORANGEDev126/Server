#pragma once

namespace network { class RIOSocket; }

namespace network
{
class RIOThreadContainer
{
public:
	static RIOThreadContainer* GetInstance()
	{
		static RIOThreadContainer container;
		return &container;
	}

	class PollingThreadContainer
	{
	public:
		struct ThreadSlot
		{
			RIO_CQ cq_;
			std::shared_ptr<std::mutex> mutex_;
			std::shared_ptr<std::thread> thread_;
		};

		~PollingThreadContainer();

		void StartWorkerThread(int thread_count);
		void WorkerThread(RIO_CQ cq, std::shared_ptr<std::mutex> mutex);
		RIO_RQ BindSocket(const std::shared_ptr<RIOSocket>& socket);
		void Stop();
		bool IsRunning() const { return !stop_; }

	private:
		std::atomic_uint64_t round_robin_{ 0 };
		std::vector<ThreadSlot> slot_;
		std::atomic_bool stop_{ true };
	};

	class IOCPThreadContainer
	{
	public:
		~IOCPThreadContainer();

		void StartWorkThread(int thread_count);
		void WorkerThread() const;
		void BindSocket(const std::shared_ptr<RIOSocket>& socket);
		void Stop();
		bool IsRunning() const { return !stop_; }

	private:
		HANDLE iocp_;
		RIO_CQ rio_cq_;
		std::vector<std::thread> thread_;
		std::mutex mutex_;
		std::atomic_bool stop_{ true };
	};

	void WorkerThread(RIO_CQ cq);
	void StartThread();
	void StopThread();
	RIO_RQ BindSocket(SOCKET rawSock, const std::shared_ptr<RIOSocket>& socket);

private:
	RIOThreadContainer() = default;
	~RIOThreadContainer() = default;

	PollingThreadContainer polling_container_;
	IOCPThreadContainer iocp_container_;
};

void DoIOCallBack(RIORESULT* result, int size);

}