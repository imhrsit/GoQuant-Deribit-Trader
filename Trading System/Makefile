CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
JSONCPP_CFLAGS = $(shell pkg-config --cflags jsoncpp)
JSONCPP_LIBS = $(shell pkg-config --libs jsoncpp)

# Add curl library and nlohmann-json
LIBS = -lcurl $(JSONCPP_LIBS)

SOURCES = main.cpp helper.cpp
TARGET = trading_system

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(JSONCPP_CFLAGS) -I/opt/homebrew/include -o $(TARGET) $(SOURCES) $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: clean 