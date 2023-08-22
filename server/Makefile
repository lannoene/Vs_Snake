# A simple Makefile for compiling small SDL projects
# paste this in cxxflags (#2) & in ldflags (#2) for packages `pkg-config --libs $(PACKAGES)`
# set the compiler flags
PACKAGES := 
CXXFLAGS := -Og -Wall -Wextra -std=gnu++20 -Wno-unused-parameter
CXXFLAGS += 
LDFLAGS := -static
LDFLAGS += -lws2_32
# add header files here
HDRS :=
BUILDIR := build

# add source files here
SRCS := source/main.cpp source/game.cpp source/network.cpp source/player.cpp source/frame_cap.cpp source/collectable.cpp

# generate names of object files
OBJS := $(addprefix $(BUILDIR)/, $(SRCS:.cpp=.o))
DEPS := $(OBJS:.o=.d)

# default recipe
all: output

# recipe for building the final executable
output: $(OBJS) $(HDRS) Makefile
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

main.cpp: titlescreen.hpp

$(BUILDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -c -o $@ $<

# recipe for building object files
#$(OBJS): $(@:.o=.c) $(HDRS) Makefile
#    $(CC) -o $@ $(@:.o=.c) -c $(CFLAGS)

# recipe to clean the workspace
clean:
	rm -f $(EXEC) $(DEPS) $(OBJS)

.PHONY: all clean

-include $(DEPS)
