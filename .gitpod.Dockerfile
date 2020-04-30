FROM debian:latest
                    
USER gitpod

RUN sudo apt-get -q update && sudo apt-get install -yq libreadline-dev gcc make curl zsh
RUN zsh -c "$(curl -fsSL https://raw.githubusercontent.com/robbyrussell/oh-my-zsh/master/tools/install.sh)" || true
RUN ln -f /bin/zsh /bin/sh