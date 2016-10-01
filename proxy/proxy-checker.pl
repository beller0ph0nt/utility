#!/usr/bin/perl -w

# Proxy list tester using threads
#
# INPUT file -> "proxy"
# format -> IP:PORT on each row
#
# OUTPUT file -> "proxy.log"

use threads;
use WWW::Mechanize;

$nn = 50; # number of simultaneous connections
$timeout = 10; # proxy request timeout (secs)

sub start_thread
{
    my @args = @_;
    my $go = WWW::Mechanize->new(agent => "Mozilla/5.0", timeout => $timeout);

    $go->proxy(['http'], 'http://'.$args[0].'/');
    print "Testing: ".$args[0]."\n";

    eval
    {
#        $go->get('http://automation.whatismyip.com/n09230945.asp');
        $go->get('http://rutracker.org');
    };

    print "Check eval\n";
    if ($@)
    {
        print "Eval error!\n";
        return;
    }

    $match = $go->content;

    print "Context: ".$match."\n";
    print "args[0]: ".$args[0]."\n";

    my ($crap, $ip) = split(/^(.*):/, $args[0]);

    print "crap: ".$crap."\n";
    print "ip: ".$ip."\n";

    if ($match =~ /$ip/)
    {
        open(LOG,">> proxy.log");
        print LOG $args[0]."\n";
        close(LOG);
        print "======> UP $args[0] <=======\n";
    }
}

@proxy = `cat proxy`;
$n = 0;

foreach $i (@proxy)
{
    chomp($i);
    $thr[$n] = threads->create('start_thread', $i);
    $n++;
    if ($n == $nn)
    {
        `sleep $timeout`;
        for ($x=0; $x<$nn;$x++)
        {
            $thr[$x]->join();
        }
        $n = 0;
    }
}