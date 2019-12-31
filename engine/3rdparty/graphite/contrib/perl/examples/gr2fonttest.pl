#!/usr/bin/perl
use Encode qw/encode/;
    $SIG{__WARN__} = sub {
        CORE::dump if $_[0] =~ /uninitialized value in subroutine entry/;
        warn @_;
    };

use strict;
#use warnings;
use Text::Gr2;
use Getopt::Long;
my %opts;
my $mask = 0x80 | 0x02;

GetOptions(\%opts, "trace=s", "mask=i","cache","dpi=i","point=i");

my ($fontfile, $text) = @ARGV;
die "No font file given!\n" unless $fontfile;
die "Font file not readable!\n" unless -r $fontfile;

if ($opts{trace}) {
    $mask = $opts{mask} if defined $opts{mask};
    open my $t, ">", $opts{trace} or die "Can't write on trace file!\n";
    Text::Gr2->start_logging($t, $mask);
}

my $face = Text::Gr2::Face->open($fontfile,
    $opts{cache} ? (cache_size => 1000, dumb_rendering => 1) : (),
    preload_glyphs => 1);
die "Invalid font, failed to read or parse tables\n" unless $face;

if (!$text) {
    print_features($face);
    exit;
}

my $point = $opts{point} // 12;
my $dpi = $opts{dpi} // 72;
my $sized_font = $face->make_font($dpi/72*$point);

my $seg = $sized_font->segment($face, $text);
print "pos  gid   attach\t     x\t     y\tins bw\t chars\t\tUnicode\n";
my $p = 0;
my %map;
my @slots = $seg->slots;
for (@slots) { $map{0+$_} = $p++ }
$p = 0;
for my $slot ($seg->slots) {
    printf("%02d  %4d %3d\@%d,%d\t%6.1f\t%6.1f\t%2d%4d\t%3d %3d\t",
            $p++, $slot->gid, $map{$slot->attached_to} || -1,
            $slot->attr($seg, 3,0),
            $slot->attr($seg, 4,0),
            $slot->origin_X,
            $slot->origin_Y,
            $slot->can_insert_before,
            $seg->cinfo($slot->original)->break_weight,$slot->before, $slot->after,
   );
   print "\n";
}

print "\nChar\tUnicode\n";
$p = 0;
my @cinfos = $seg->cinfos;
for (@cinfos) {
    printf "%d\t%04X\n", $p++,$_->unicode_char();
}
sub print_features {
    my $face = shift;
    print $face->n_fref." features\n";
    for my $f ($face->features) {
        my $label = $f->label(0x0409, 1);
        my $fuid = $f->id;
        print $f->_id." ".$fuid." ".$label."\n";
        for ($f->values) {
            my $label = $f->value_label($_,0x0409, 1);
            print "\t$_\t$label\n";
        }
        print "\n";
    }
    print "Feature languages: ",join "\t", $face->languages;
    print "\n";
}
END { Text::Gr2->stop_logging; }
