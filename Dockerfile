FROM alpine:latest

WORKDIR /app

ENV DEVKITPRO=/opt/devkitpro
ENV DEVKITARM=/opt/devkitpro/devkitARM
ENV PATH=/opt/devkitpro/tools/bin:$PATH

# Install base packages
RUN apk update && \
    apk upgrade && \
    apk add --no-cache \
        bash \
        curl \
        git \
        make \
        cmake \
        zip \
        unzip \
        tar \
        xz

# Download and extract devkitARM manually
# Note: You may need to download from an alternative source or use wget with proper headers
RUN mkdir -p /opt/devkitpro && \
    cd /opt/devkitpro && \
    curl -L https://github.com/devkitPro/pacman/releases/latest/download/devkitpro-pacman.deb -o /tmp/devkitpro-pacman.deb

# Copy project files
COPY . .

# Build the project
RUN make

ENTRYPOINT ["sh"]
