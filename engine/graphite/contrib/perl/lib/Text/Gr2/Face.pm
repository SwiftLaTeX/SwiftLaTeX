package Text::Gr2::Face;

=head1 NAME

Text::Gr2::Face - Open and manipulate Graphite font files

=head1 METHODS

=head2 open

    Text::Gr2::Face->open( $font, %options )

This is the main entry point to Graphite2. It expects the full pathname
of a Graphite-enabled TrueType font. (Note: only about three or four fonts
right now are Graphite enabled.)

=cut

sub open {
    my ($self, $ff, %opts) = @_;
    my $opt = 0;
    $opt |= 1 if $opts{dumb_rendering};
    $opt |= 2 if $opts{preload_glyphs};
    $opt |= 4 if $opts{cache_cmap};

    if ($opts{cache_size}) {
        $self->make_file_face_with_seg_cache($ff,$opts{cache_size},$opt);
    } else {
        $self->make_file_face($ff,$opt);
    }
}

=head2 features

Returns a list of L<Gr2::FeatureRef> objects representing features
that this font possesses.

=cut

sub features {
    my $self = shift;
    map { $self->fref($_) } (0..$self->n_fref() -1)
}

=head2 languages

Returns a list of language codes that the font services.

=cut

sub languages {
    my $self = shift;
    map { Text::Gr2::_decode($self->lang_by_index($_)) } 0..$self->n_languages-1;
}

=head2 make_font

    $font = $face->make_font($dpi*$pointsize/72)

Scales a face to a given font size, by point size/DPI.

=head1 INTERNAL METHODS

=head2 featureval_for_lang

Get feature values for a given language or default

=head2 n_fref

Return the number of feature references in this face.

=head2 fref

Return a given feature reference by index.

=head2 n_languages

Return the number of languages that this face supports.

=head2 lang_by_index

Return a given language code by index.

=head2 find_fref

Get feature reference for a given feature id from a face

=head2 make_file_face

=head2 make_file_face_with_seg_cache

Low-level methods to create a face from a font file.

=head2 n_glyphs

Return the number of glyphs in this face.

=cut

1;
