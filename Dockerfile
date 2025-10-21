FROM devkitpro/devkita64:latest AS builder

WORKDIR /app

# Copy project files
COPY . .

# Build the mod
RUN make

# List what was built (for debugging)
RUN find /app -name "*.nso" -o -name "*.elf" -o -name "main.npdm" -o -name "subsdk9"

# Second stage
FROM alpine:latest
WORKDIR /output

# Copy specific known files
COPY --from=builder /app/build/subsdk9 /output/
COPY --from=builder /app/build/main.npdm /output/

ENTRYPOINT ["sh"]
