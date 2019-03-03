FROM hydai/ejudge-base:latest

LABEL maintainer="hydai@skymizer.com"

# Copy ejudge into container
RUN mkdir -p /workdir
WORKDIR /workdir
ADD . /workdir
