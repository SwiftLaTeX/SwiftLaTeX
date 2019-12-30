package Text::Gr2::Segment;

=head1 NAME

Text::Gr2::Segment - Represent a segment of text

=head1 METHODS

=head2 slots

Returns all of the character slots in this segment

=head2 cinfos

Returns C<Text::Gr2::CharInfo> objects for all character in this
segment.

=head2 first_slot

Returns the first C<Text::Gr2::Slot> object in this segment.

=head2 last_slot

You can guess, I think.

=head2 advance_X

Returns the advance for the whole segment.

=head2 advance_Y

Returns the advance height for the whole segment.

=head1 INTERNAL METHODS

=head2 cinfo

Return a C<CharInfo> object for a given index.

=head2 n_cinfo

Tells you how many character objects there are.


=cut

sub slots {
    my $self = shift;
    my @slots;
    my $s = $self->first_slot;
    while ($s) { push @slots, $s; $s = $s->next_in_segment }
    return @slots;
}

sub cinfos { map { $_[0]->cinfo($_) } 0..($_[0]->n_cinfo -1) }

1;
