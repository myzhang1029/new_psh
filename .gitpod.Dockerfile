FROM gitpod/workspace-full:latest

USER gitpod

RUN sudo apt-get -q update && sudo apt-get install -yq libreadline-dev \
libgoogle-perftools-dev google-perftools gcc make curl zsh
RUN zsh -c "$(curl -fsSL https://raw.githubusercontent.com/robbyrussell/oh-my-zsh/master/tools/install.sh)"
RUN chsh -s /usr/bin/zsh gitpod
