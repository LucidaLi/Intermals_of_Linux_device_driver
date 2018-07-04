#include <fcntl.h>
#include <libaio.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

#define FLAGS_file_size	0x1000
#define concurrent_requests 100
#define FLAGS_min_nr  1
#define FLAGS_max_nr  1

// The size of operation that will occur on the device
static const int kPageSize = 4096;

class AIORequest {
 public:
  int* buffer_;

  virtual void Complete(int res) = 0;

  AIORequest() {
    int ret = posix_memalign(reinterpret_cast<void**>(&buffer_),
                             kPageSize, kPageSize);
  }

  virtual ~AIORequest() {
    free(buffer_);
  }
};

class Adder {
 public:
  virtual void Add(int amount) = 0;

  virtual ~Adder() { };
};

class AIOReadRequest : public AIORequest {
 private:
  Adder* adder_;

 public:
  AIOReadRequest(Adder* adder) : AIORequest(), adder_(adder) { }
  virtual void Complete(int res) {
    int value = buffer_[0];
    cout << "Read of " << value << " completed" << endl;
    adder_->Add(value);
  }
};

class AIOWriteRequest : public AIORequest {
 private:
  int value_;

 public:
  AIOWriteRequest(int value) : AIORequest(), value_(value) {
    buffer_[0] = value;
  }

  virtual void Complete(int res) {
    cout << "Write of " << value_ << " completed" << endl;
  }
};

class AIOAdder : public Adder {
 public:
  int fd_;
  io_context_t ioctx_;
  int counter_;
  int reap_counter_;
  int sum_;
  int length_;

  AIOAdder(int length)
      : ioctx_(0), counter_(0), reap_counter_(0), sum_(0), length_(length) { }

  void Init() {
    cout << "Opening file" << endl;
    fd_ = open("/tmp/testfile", O_RDWR | O_DIRECT | O_CREAT, 0644);
    if (fd_ == -1) {
        cout << "open file failed" << endl;
	return;
    }
    
    if (fallocate(fd_, 0, 0, kPageSize * length_) != 0) {
        cout << "fallocate failed" << endl;
	return;
    }    

    cout << "Allocating enough space for the sum" << endl;
    cout << "Setting up the io context" << endl;
    if (io_setup(100, &ioctx_) != 0) {
        cout << "io_setup failed" << endl;
	return;
    }
  }

  virtual void Add(int amount) {
    sum_ += amount;
    cout << "Adding " << amount << " for a total of " << sum_ << endl;
  }
 void SubmitWrite() {
    cout << "Submitting a write to " << counter_ << endl;
    struct iocb iocb;
    struct iocb* iocbs = &iocb;
    AIORequest *req = new AIOWriteRequest(counter_);
    io_prep_pwrite(&iocb, fd_, req->buffer_, kPageSize, counter_ * kPageSize);
    iocb.data = req;
    int res = io_submit(ioctx_, 1, &iocbs);
  }

  void WriteFile() {
    reap_counter_ = 0;
    for (counter_ = 0; counter_ < length_; counter_++) {
      SubmitWrite();
      Reap();
    }
    ReapRemaining();
  }

  void SubmitRead() {
    cout << "Submitting a read from " << counter_ << endl;
    struct iocb iocb;
    struct iocb* iocbs = &iocb;
    AIORequest *req = new AIOReadRequest(this);
    io_prep_pread(&iocb, fd_, req->buffer_, kPageSize, counter_ * kPageSize);
    iocb.data = req;
    int res = io_submit(ioctx_, 1, &iocbs);
  }

  void ReadFile() {
    reap_counter_ = 0;
    for (counter_ = 0; counter_ < length_; counter_++) {
        SubmitRead();
        Reap();
    }
    ReapRemaining();
  }

  int DoReap(int min_nr) {
    cout << "Reaping between " << min_nr << " and "
              << FLAGS_max_nr << " io_events" << endl;
    struct io_event* events = new io_event[FLAGS_max_nr];
    struct timespec timeout;
    timeout.tv_sec = 0;
    timeout.tv_nsec = 100000000;
    int num_events;
    cout << "Calling io_getevents" << endl;
    num_events = io_getevents(ioctx_, min_nr, FLAGS_max_nr, events,
                              &timeout);
    cout << "Calling completion function on results" << endl;
    for (int i = 0; i < num_events; i++) {
      struct io_event event = events[i];
      AIORequest* req = static_cast<AIORequest*>(event.data);
      req->Complete(event.res);
      delete req;
    }
    delete events;
    
    cout << "Reaped " << num_events << " io_events" << endl;
    reap_counter_ += num_events;
    return num_events;
  }

  void Reap() {
    if (counter_ >= FLAGS_min_nr) {
      DoReap(FLAGS_min_nr);
    }
  }

  void ReapRemaining() {
    while (reap_counter_ < length_) {
      DoReap(1);
    }
  }

  ~AIOAdder() {
    cout << "Closing AIO context and file" << endl;
    io_destroy(ioctx_);
    close(fd_);
  }

  int Sum() {
    cout << "Writing consecutive integers to file" << endl;
    WriteFile();
    cout << "Reading consecutive integers from file" << endl;
    ReadFile();
    return sum_;
  }
};

int main(int argc, char* argv[]) {
  AIOAdder adder(FLAGS_file_size);
  adder.Init();
  int sum = adder.Sum();
  int expected = (FLAGS_file_size * (FLAGS_file_size - 1)) / 2;
  cout << "AIO is complete" << endl;
  printf("Successfully calculated that the sum of integers from 0"
         " to %d is %d\n", FLAGS_file_size - 1, sum);
  return 0;
}

