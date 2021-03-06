#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <check.h>
#include "check_parse_phylip.h"
#include "helper_methods.h"
#include "parse_phylip.h"


START_TEST (phylip_read_in_small_file)
{
  load_sequences_from_multifasta_file("../tests/data/small_phylip_file.aln");

  fail_unless( number_of_samples_from_parse_phylip() == 3);
  fail_unless( find_sequence_index_from_sample_name("2956_6_1") == 0);
  fail_unless( find_sequence_index_from_sample_name("2956_6_2") == 1);
  fail_unless( find_sequence_index_from_sample_name("2956_6_3") == 2);
  
  char *sample_names[3];
  get_sample_names_from_parse_phylip(sample_names);
  fail_unless( strcmp(sample_names[0],"2956_6_1") == 0 );
  fail_unless( strcmp(sample_names[1],"2956_6_2") == 0 );
  fail_unless( strcmp(sample_names[2],"2956_6_3") == 0 );

  char *reference_bases = "*ACG*";
  char *filtered_bases_for_snps[3];

  filter_sequence_bases_and_rotate(reference_bases, filtered_bases_for_snps, 3);
  fail_unless( strcmp(filtered_bases_for_snps[0], "AAT") == 0 );
  fail_unless( strcmp(filtered_bases_for_snps[1], "CGT") == 0 );
  fail_unless( strcmp(filtered_bases_for_snps[2], "GGT") == 0 );
  

  
  fail_unless( does_column_contain_snps(0, 'A') == 0);
  fail_unless( does_column_contain_snps(1, 'A') == 1);
  fail_unless( does_column_contain_snps(2, 'A') == 1);
  // bad reference base
  fail_unless( does_column_contain_snps(0, 'X') == 1);
  
  char sequence_bases[10];
  get_sequence_for_sample_name(sequence_bases, "2956_6_2");
  fail_unless( strcmp(sequence_bases, "AAGGC") == 0);
  
  update_sequence_base('X', 1, 4);
  get_sequence_for_sample_name(sequence_bases, "2956_6_2");
  fail_unless( strcmp(sequence_bases, "AAGGX") == 0);
  
}
END_TEST

START_TEST (phylip_read_in_file_with_gaps)
{
	load_sequences_from_multifasta_file("../tests/data/alignment_with_gaps.aln");
	fail_unless( does_column_contain_snps(0, 'A') == 0);
  fail_unless( does_column_contain_snps(1, '-') == 0);
  fail_unless( does_column_contain_snps(2, '-') == 0);
  fail_unless( does_column_contain_snps(3, 'T') == 0);
  fail_unless( does_column_contain_snps(4, 'G') == 1);
  fail_unless( does_column_contain_snps(4, '-') == 1);
  fail_unless( does_column_contain_snps(5, 'N') == 0);

  // Check you can fill in parent bases with gaps if all children have the same gap at the same place
	int child_indices[2] = {1,2};
	fill_in_unambiguous_gaps_in_parent_from_children(0, child_indices, 2);
	char sequence_bases[10];
  get_sequence_for_sample_name(sequence_bases, "2956_6_1");
  fail_unless( strcmp(sequence_bases, "A-N-CT") == 0);
}
END_TEST

START_TEST (phylip_fill_in_unambiguous_bases_in_parent_from_children_where_parent_has_a_gap)
{
	load_sequences_from_multifasta_file("../tests/data/alignment_with_gap_in_parent.aln");
	int child_indices[2] = {0,1};
	fill_in_unambiguous_bases_in_parent_from_children_where_parent_has_a_gap(2, child_indices, 2);
	char sequence_bases[10];
  get_sequence_for_sample_name(sequence_bases, "parent");
  fail_unless( strcmp(sequence_bases, "AC--") == 0);
}
END_TEST



Suite * parse_phylip_suite(void)
{
  Suite *s = suite_create ("Parsing a phylip file");
  TCase *tc_phylip = tcase_create ("phylip_files");
  tcase_add_test (tc_phylip, phylip_read_in_small_file);
  tcase_add_test (tc_phylip, phylip_read_in_file_with_gaps);
  tcase_add_test (tc_phylip, phylip_fill_in_unambiguous_bases_in_parent_from_children_where_parent_has_a_gap);
  suite_add_tcase (s, tc_phylip);
  return s;
}
