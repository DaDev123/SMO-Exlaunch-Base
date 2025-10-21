FROM devkitpro/devkita64:latest AS builder

WORKDIR /app

# Copy project files
COPY . .

# Ensure git is installed, then initialize and update submodules
RUN apt-get update && \
    apt-get install -y git && \
    rm -rf /var/lib/apt/lists/* && \
    git config --global --add safe.directory /app && \
    git submodule update --init --recursive

# Build the mod
RUN make

# List what was built (for debugging)
RUN find /app -name "*.nso" -o -name "*.elf" -o -name "main.npdm" -o -name "subsdk9"

# Second stage
FROM alpine:latest
WORKDIR /output

COPY --from=builder /app/build /output/

ENTRYPOINT ["sh"]
