#!/bin/bash


# You can do something like this:
#
#     export http_proxy=PROXY_TO_CHECK
#     wget www.google.com
#
# You can try to get a webpage using the proxy and if that fails,
# the proxy is no good. You could put this in a script too if you wanted to...





function proxy_on()
{
    export no_proxy="localhost,127.0.0.1,localaddress,.localdomain.com"

    if (( $# > 0 ))
    then
#        valid=$(echo $@ | sed -n 's/\([0-9]\{1,3\}.\)\{4\}:\([0-9]\+\)/&/p')
#        if [[ $valid != $@ ]]; then
#            >&2 echo "Invalid address"
#            return 1
#        fi

        export http_proxy="http://$1/"
        export https_proxy=$http_proxy
        export ftp_proxy=$http_proxy
        export rsync_proxy=$http_proxy
        echo "Proxy environment variable set."

        return 0
    fi

#    echo -n "username: "; read username
#    if [[ $username != "" ]]; then
#        echo -n "password: "
#        read -es password
#        local pre="$username:$password@"
#    fi

#    echo -n "server: "; read server
#    echo -n "port: "; read port
#    export http_proxy="http://$pre$server:$port/"
#    export https_proxy=$http_proxy
#    export ftp_proxy=$http_proxy
#    export rsync_proxy=$http_proxy
#    export HTTP_PROXY=$http_proxy
#    export HTTPS_PROXY=$http_proxy
#    export FTP_PROXY=$http_proxy
#    export RSYNC_PROXY=$http_proxy
}

function proxy_off()
{
    unset http_proxy
    unset https_proxy
    unset ftp_proxy
    unset rsync_proxy
    echo -e "Proxy environment variable removed."
}


#valid=$(echo $@ | sed -n 's/\([0-9]\{1,3\}.\)\{4\}:\([0-9]\+\)/&/p')

#echo "valid = "$valid


proxy_list=`cat $1`

#echo "list:"
#echo $list

for proxy in $proxy_list
do
    echo $proxy
    proxy_on $proxy

    break
done

p=`env | grep 'proxy'`
echo $p


