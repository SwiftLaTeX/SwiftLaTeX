package Text::Gr2::FeatureRef;

=head1 NAME

Text::Gr2::FeatureRef - References to font features

=head1 METHODS

=head2 values

Returns all possible values for this feature

=head2 id

Returns the tag name of the features

=cut

sub values {
    my $self = shift;
    map { $self->value($_) } (0..$self->n_values - 1);
}

sub id { Text::Gr2::_decode(shift->_id) }

=head2 label

    $lab = $self->label(Gr2->str_to_tag($language), $encoding)

Returns an explanatory string for this font feature, in the given
language and encoding format. Typically you want the label to be
returned in UTF-8, which is encoding format 1.

=head2 value_label

Returns an explanatory string for a given value of this font feature.

    $lab = $self->value_label($setting_no,
        Gr2->str_to_tag($language), $encoding)

Please note that this takes an integer setting number, not a FeatureVal
object.

Reading the C<print_features> subroutine of F<examples/gr2fonttest.pl>
will make this all bit more clear.

=head1 INTERNAL METHODS

=head2 value

Returns a given value by index.

=head2 n_values

Returns the number of feature values.

=head2 feature_value

Returns the current value of this feature.

=head2 set_feature_value

Sets the valuse of this feature.

=cut



1;
