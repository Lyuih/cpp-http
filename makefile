CXX := g++
CXXFLAGS := -std=c++17 -Wall
LIBS := -lpthread
SRCS := main.cc TcpServer.cc HttpServer.cc ThreadPool.cc HttpRequest.cc HttpResponse.cc Buffer.cc Epoller.cc
TARGET := main

$(TARGET) : $(SRCS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -rf $(TARGET)