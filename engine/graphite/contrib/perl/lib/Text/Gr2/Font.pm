package Text::Gr2::Font;
use strict;
no bytes;
use Encode qw/_utf8_on/;

=head1 NAME

Text::Gr2::Font - Font handling methods

=head1 METHODS

=head2 segment

    my $seg = $font->segment($face, $text,
        # Some optional arguments:
            encoding => $enc, # "utf8" || "utf16" || "utf32"
            features => $features,
            script   => $script_tag
    );

Makes a L<Text::Gr2::Segment> object from the given text. This is
what you'll need to do to set text in this font.

=cut

sub segment {
    my ($self, $face, $text, %options) = @_;
    my $encoding = $options{encoding} || "utf8";
    if ($encoding eq "utf8") { _utf8_on($text) } # In case it isn't already
    my $a;
    my $enc = $encoding =~ /8/ ? 1 :
              $encoding =~ /16/ ? 2 :
              $encoding =~ /32/ ? 4 : 1;
    my $script = $options{script} ? Text::Gr2->str_to_tag($options{script}) : 0;
    my $pFeats = $options{features} || (bless \($a=0), "Text::Gr2::FeatureVal");
    $self->make_seg($face, $script, $pFeats, $enc, $text, length($text)/$enc, $options{rtl});
}

=head1 INTERNAL METHODS

=head2 make_seg

Underlying C interface to make segments. Use C<segment> instead.

=cut

1;
