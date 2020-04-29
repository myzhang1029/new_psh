FROM debian
                    
USER gitpod

RUN sudo apt-get -q update && sudo apt-get install -yq libreadline-dev gcc make