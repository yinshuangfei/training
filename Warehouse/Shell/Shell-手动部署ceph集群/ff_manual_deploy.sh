#!/bin/bash 

host_connect(){
    if [ $# -lt 1 ]; then
        echo "please input the initial mon host."
        exit 1
    fi

    hosts=$@
    ip=""
    for i in $hosts
    do
        hostip=$(timeout 1 ping -c 1 $i 2>/dev/null | head -n 1 | awk '{print $3}')
        hostip=${hostip#(}
        hostip=${hostip%)}

        if [ "$hostip" = "" ] ; then
            echo "The host:$i can not reachable."
            exit 1
        fi
        ip="$ip $hostip"
    done
    echo $ip
    return 0
}

_initial(){
    # get the host ip
    host_connect $@ 
    ips=$(host_connect $@)
    hostnames=$@
    
    # TODO only surpport one mon
    hostip=${ips%% *}
    hostname=${hostnames%% *}

    # generate cluster uuid
    fsid=$(uuidgen)

    touch ceph.conf
    echo "[global]
fsid = $fsid
mon initial members = $hostname
mon host = $hostip
auth cluster required = cephx
auth service required = cephx
auth client required = cephx
public network = ${hostip}/16
cluster network = ${hostip}/16
rbd default features = 1
osd pool default size = 2 " > ceph.conf

    # generate mon keyring
    ceph-authtool --create-keyring ceph.mon.keyring --gen-key -n mon. --cap mon 'allow *'

    # generate ceph.client.admin.keyring 
    ceph-authtool --create-keyring ceph.client.admin.keyring --gen-key -n client.admin --cap mon 'allow *' --cap mgr 'allow *' --cap osd 'allow *' --cap mds 'allow *'
    
    # importing contents of ceph.client.admin.keyring into ceph.mon.keyring
    ceph-authtool ceph.mon.keyring --import-keyring ceph.client.admin.keyring

    # create the monmap
    monmaptool --create --add $hostname $hostip --fsid $fsid --clobber monmap
    
    # ==========================================================================
    # cpoy the needed file to the path
    scp ceph.conf $hostip:/etc/ceph/
    scp ceph.client.admin.keyring $hostip:/etc/ceph/

    scp ceph.mon.keyring $hostip:/tmp/  
    scp monmap $hostip:/tmp/
    
    # make mon fs
    ssh $hostip ceph-mon --mkfs -i $hostname --monmap /tmp/monmap --keyring /tmp/ceph.mon.keyring 
    
    # write 'done' file to the mon path
    ssh $hostip touch /var/lib/ceph/mon/ceph-${hostname}/done 
    ssh $hostip chown -R ceph:ceph /var/lib/ceph/mon/ceph-${hostname}

    ssh $hostip systemctl enable ceph-mon.target
    ssh $hostip systemctl enable ceph-mon@${hostname}.service
    ssh $hostip systemctl start ceph-mon@${hostname}.service
}

install_mon(){
    host_connect $@
    ips=$( host_connect $@ )
    
    hostnames=$@
    # TODO only surpport one mon
    hostip=${ips%% *}
    hostname=${hostnames%% *}
    
    # TODO the second mon can not start.
    # cpoy the needed file to the path
    scp ceph.conf $hostip:/etc/ceph/
    scp ceph.client.admin.keyring $hostip:/etc/ceph/

    scp ceph.mon.keyring $hostip:/tmp/  
    scp monmap $hostip:/tmp/
    
    # make mon fs
    ssh $hostip ceph-mon --mkfs -i $hostname --monmap /tmp/monmap --keyring /tmp/ceph.mon.keyring 
    
    # write 'done' file to the mon path
    ssh $hostip touch /var/lib/ceph/mon/ceph-${hostname}/done 
    ssh $hostip chown -R ceph:ceph /var/lib/ceph/mon/ceph-${hostname}
    
    ssh $hostip systemctl enable ceph-mon.target
    ssh $hostip systemctl enable ceph-mon@${hostname}.service
    ssh $hostip systemctl start ceph-mon@${hostname}.service
}

install_mgr(){
    host_connect $@
    ips=$( host_connect $@ )

    hostnames=$@
    # TODO only surpport one mon
    hostip=${ips%% *}
    hostname=${hostnames%% *}

    # cpoy the needed file to the path
    scp ceph.conf $hostip:/etc/ceph/
    scp ceph.client.admin.keyring $hostip:/etc/ceph/

    ssh $hostip mkdir /var/lib/ceph/mgr/ceph-${hostname} 
    ssh $hostip "ceph auth get-or-create mgr.${hostname} mon 'allow profile mgr' osd 'allow *' mds 'allow *' -o /var/lib/ceph/mgr/ceph-${hostname}/keyring" 

    ssh $hostip touch /var/lib/ceph/mgr/ceph-${hostname}/done 
    ssh $hostip chown -R ceph:ceph /var/lib/ceph/mgr

    ssh $hostip systemctl enable ceph-mgr.target
    ssh $hostip systemctl enable ceph-mgr@${hostname}.service
    ssh $hostip systemctl start ceph-mgr@${hostname}.service
}

uninstall_mgr(){
    host_connect $@
    ips=$( host_connect $@ )

    hostnames=$@
    # TODO only surpport one mon
    hostip=${ips%% *}
    hostname=${hostnames%% *}
    
    ssh $hostip systemctl stop ceph-mgr@${hostname}.service
    ssh $hostip systemctl disable ceph-mgr@${hostname}.service
    ssh $hostip systemctl disable ceph-mgr.target

    ceph auth del mgr.${hostname}
    ssh $hostip rm -fr /var/lib/ceph/mgr/ceph-${hostname} 
}


install_osd(){
    host_connect $1
    ips=$( host_connect $1 )

    hostnames=$1
    # TODO only surpport one mon
    hostip=${ips%% *}
    
    shift
    devices=$@
    
    # cpoy the needed file to the path
    #scp ceph.conf $hostip:/etc/ceph/
    #scp ceph.client.admin.keyring $hostip:/etc/ceph/
    
    for dev in $devices
    do
        uuid=$(uuidgen)
        echo uuid=$uuid
        echo dev=$dev
        #ceph osd create $uuid  
    done

    #ceph osd create

}

uninstall_osd(){
    host_connect $@
    ips=$( host_connect $@ )

    hostnames=$@
    # TODO only surpport one mon
    hostip=${ips%% *}

}


_purge(){
    host_connect $@
    ips=$( host_connect $@ )

    for ip in $ips
    do 
        echo "Current hostname is : $ip"
        daemons=$(ssh $ip ls /run/ceph/)
        for daemon in $daemons
        do
            daemon_host=${daemon#ceph-}
            daemon_host=${daemon_host%.asok}

            sub_daemon=$(echo $daemon_host | awk -F "." '{print $1}')
            sub_host=$(echo $daemon_host | awk -F "." '{print $2}')
            
            ssh $ip systemctl stop ceph-${sub_daemon}@${sub_host}.service
        done
        
        if ssh $ip ls /etc/ceph/ceph.conf 2>/dev/null ; then
            ssh $ip rm /etc/ceph/ceph.conf
        fi
        if ssh $ip ls /etc/ceph/ceph.client.admin.keyring 2>/dev/null ; then
            ssh $ip rm /etc/ceph/ceph.client.admin.keyring 
        fi

        ssh $ip 'for dir in $(ls /var/lib/ceph/);do rm -fr /var/lib/ceph/${dir}/* ;done'
    done 
}


_install(){
    case "$1" in
    mon)
        shift
        install_mon $@
        ;;
    mgr)
        shift
        install_mgr $@
        ;;
    mds)
        shift
        install_mds $@
        ;;
    osd)
        shift
        install_osd $@
        ;;
    *)
        echo "usage: install mon | mgr | mds | osd"
    esac
}

_uninstall(){
    case "$1" in
    mon)
        shift
        uninstall_mon $@
        ;;
    mgr)
        shift
        uninstall_mgr $@
        ;;
    mds)
        shift
        uninstall_mds $@
        ;;
    osd)
        shift
        uninstall_osd $@
        ;;
    *)
        echo "usage: uninstall mon | mgr | mds | osd"
    esac
}

case "$1" in  
    init)
        shift
        _initial $@
        ;;
    install)
        shift
        _install $@
        ;;
    uninstall)
        shift
        _uninstall $@
        ;;
    purge)
        shift
        _purge $@
        ;;
    *)  
        echo "usage: $0 { init | install | uninstall | purge }"
        echo -e "\t\t init HOST                          \t\t: initial a cluster with a host."
        echo -e "\t\t install <mon|mgr|mds|mgr> HOST     \t\t: install a deamon in a host."
        echo -e "\t\t uninstall <mon|mgr|mds|mgr> HOST   \t\t: uninstall a deamon in a host."
        echo -e "\t\t purge HOST                         \t\t: purge a cluster on a host."
        ;;  
esac
