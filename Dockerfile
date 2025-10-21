FROM devkitpro/devkita64:latest AS builder

WORKDIR /app

# Copy project files
COPY . .

# Build the mod
RUN make

# Second stage - just copy the compiled output
FROM alpine:latest
WORKDIR /output
COPY --from=builder /app/atmosphere /output/atmosphere
COPY --from=builder /app/*.nso /output/ 2>/dev/null || :
COPY --from=builder /app/*.elf /output/ 2>/dev/null || :

ENTRYPOINT ["sh"]
