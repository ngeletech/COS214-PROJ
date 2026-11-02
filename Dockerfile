# Use a lightweight Ubuntu base
FROM ubuntu:22.04

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install C++ build tools
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory inside the container
WORKDIR /app

# Copy everything from your local project into the container
COPY . .

# Configure and build the project using CMake
# (This builds both the unit_tests and the tividy_tui executables)
RUN cmake -S . -B build
RUN cmake --build build

# Default command: run the unit tests. 
# (Docker Compose will override this to run the TUI).
CMD ["ctest", "--test-dir", "build", "--output-on-failure"]