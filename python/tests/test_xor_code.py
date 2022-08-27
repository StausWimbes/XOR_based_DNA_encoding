import xor_code_python as xor


def test_max_strand_length():
    xor.set_max_strand_length(120)
    assert xor.get_max_strand_length() == 120
