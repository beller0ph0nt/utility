#!/usr/bin/perl -w

use strict;
use threads;
use threads::shared;
use IO::Socket::INET;

my @proxy : shared;
my $threads = 8;
my $myip    = '124.110.14.213'; #тут указать свой ип

open(my $infile, '<proxy');
@proxy = <$infile>;
close($infile);
chomp(@proxy);

open(my $anon, '>>anon.txt');

my (@thread);

print "Run threads...\n";

for(0..$threads)
{
    $thread[$_] = threads->create(\&check);

}

print "Wait threads...\n";

for(0..$threads)
{
    $thread[$_]->join;
}

sub check
{
    my ($prox, $port, $sock);
    while(@proxy)
    {
        ($prox, $port) = split(/:/, shift(@proxy));
        print "Testing ip: ".$prox." port: ".$port."\n";

        $sock = IO::Socket::INET->new(Proto    => 'tcp',
                                      PeerAddr => $prox,
                                      PeerPort => $port,
                                      Timeout  => 4) || 0;
        if ($sock)
        {
            print $sock "GET http://2ip.ru/ HTTP/1.1\r\n".
                        "Host: 2ip.ru\r\n".
                        "Connection: close\r\n\r\n";
            while(<$sock>)
            {
                #print;
                if (/$myip/)
                {
                    print "Govno: $prox:$port\n";
                    last
                }

                if (/Top100 logo/)
                {
                    print $anon "$prox:$port\n"
                }
            }
        }
    }
}