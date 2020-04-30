FROM debian:latest
                    
USER gitpod

RUN sudo apt-get -q update && sudo apt-get install -yq libreadline-dev gcc make curl zsh
RUN ln -f /bin/zsh /bin/sh