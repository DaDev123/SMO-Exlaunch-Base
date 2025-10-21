FROM devkitpro/devkita64:latest AS builder

WORKDIR /app

# Copy all files including .git directory
COPY . .

# Install git if not present and init submodules
RUN apt-get update && \
    apt-get install -y git && \
    rm -rf /var/lib/apt/lists/* && \
    git config --global --add safe.directory /app && \
    git submodule update --init --recursive

# Build the mod
RUN make

# Second stage - just copy the compiled output
FROM alpine:latest
WORKDIR /output
COPY --from=builder /app/atmosphere /output/atmosphere
COPY --from=builder /app/build/*.nso /output/ 2>/dev/null || :
COPY --from=builder /app/build/*.elf /output/ 2>/dev/null || :
COPY --from=builder /app/*.nso /output/ 2>/dev/null || :
COPY --from=builder /app/*.elf /output/ 2>/dev/null || :

ENTRYPOINT ["sh"]
