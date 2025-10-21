FROM devkitpro/devkita64:latest

WORKDIR /app

# Install dependencies
RUN apt-get update && \
    apt-get install -y \
      make \
      git && \
    rm -rf /var/lib/apt/lists/*

# Copy project files
COPY . .

# Set make as the entrypoint so users can pass targets as arguments
ENTRYPOINT ["make"]
