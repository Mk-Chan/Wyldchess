#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "bitboard.h"
#include "magicmoves.h"

u64 p_atks[2][64];
u64 n_atks[64];
u64 k_atks[64];
u64 b_pseudo_atks[64];
u64 r_pseudo_atks[64];
u64 q_pseudo_atks[64];
u64 dirn_sqs[64][64];
u64 intervening_sqs[64][64];

u64 rank_mask[8] = {
	0xffULL,
	0xffULL << 8,
	0xffULL << 16,
	0xffULL << 24,
	0xffULL << 32,
	0xffULL << 40,
	0xffULL << 48,
	0xffULL << 56
};

u64 file_mask[8] = {
	0x0101010101010101ULL,
	0x0202020202020202ULL,
	0x0404040404040404ULL,
	0x0808080808080808ULL,
	0x1010101010101010ULL,
	0x2020202020202020ULL,
	0x4040404040404040ULL,
	0x8080808080808080ULL
};

u64 passed_pawn_mask[2][64];
u64 king_forward_mask[2][64];
u64 file_forward_mask[2][64];
u64 adjacent_files_mask[8];
u64 adjacent_ranks_mask[8];
u64 color_sq_mask[2];

static inline int file_diff(int sq1, int sq2)
{
	return abs((sq1 % 8) - (sq2 % 8));
}

void init_masks()
{
	int c, sq;
	for (sq = 0; sq != 8; ++sq) {
		adjacent_files_mask[sq] = 0ULL;
		adjacent_ranks_mask[sq] = 0ULL;
		if (sq != 7) {
			adjacent_files_mask[sq] |= file_mask[sq + 1];
			adjacent_ranks_mask[sq] |= rank_mask[sq + 1];
		}
		if (sq != 0) {
			adjacent_files_mask[sq] |= file_mask[sq - 1];
			adjacent_ranks_mask[sq] |= rank_mask[sq - 1];
		}
	}
	color_sq_mask[WHITE] = 0ULL;
	color_sq_mask[BLACK] = 0ULL;
	for (sq = 0; sq != 64; ++sq) {
		if (   (rank_of(sq) % 2 == 0 && sq % 2 == 0)
		    || (rank_of(sq) % 2 == 1 && sq % 2 == 1))
			color_sq_mask[WHITE] |= BB(sq);
		else
			color_sq_mask[BLACK] |= BB(sq);
	}

	int forward, i, offset;
	for (c = WHITE; c <= BLACK; ++c) {
		forward = c == WHITE ? 1 : -1;
		for (sq = 0; sq != 64; ++sq) {
			file_forward_mask[c][sq] = 0ULL;
			for (i = sq + (forward << 3); i >= 0 && i <= 63; i += forward << 3)
				file_forward_mask[c][sq] |= BB(i);
			king_forward_mask[c][sq] = 0ULL;
			offset = sq + 8 * forward;
			if (offset >= 0 && offset <= 63) {
				king_forward_mask[c][sq] = BB(offset);
				if (file_of(sq) != 0)
					king_forward_mask[c][sq] |= BB((offset - 1));
				if (file_of(sq) != 7)
					king_forward_mask[c][sq] |= BB((offset + 1));
			}
		}
	}

	for (c = WHITE; c <= BLACK; ++c) {
		for (sq = 0; sq != 64; ++sq) {
			passed_pawn_mask[c][sq] = file_forward_mask[c][sq];
			if (file_of(sq) != 0)
				passed_pawn_mask[c][sq] |= file_forward_mask[c][sq - 1];
			if (file_of(sq) != 7)
				passed_pawn_mask[c][sq] |= file_forward_mask[c][sq + 1];
		}
	}
}

void init_atks()
{
	static int king_offsets[8] = { -9, -8, -7, -1, 1, 7, 8, 9 };
	static int knight_offsets[8] = { -17, -15, -10, -6, 6, 10, 15, 17 };
	static int pawn_offsets[2][2] = { { 7, 9 }, { -9, -7 } };
	int off, ksq, nsq, psq, c,
	    sq = 0;

	for (; sq != 64; ++sq) {
		k_atks[sq] = 0ULL;
		n_atks[sq] = 0ULL;
		p_atks[WHITE][sq] = 0ULL;
		p_atks[BLACK][sq] = 0ULL;

		for (off = 0; off != 8; ++off) {
			ksq = sq + king_offsets[off];
			if (   ksq <= H8
			    && ksq >= A1
			    && file_diff(sq, ksq) <= 1)
				k_atks[sq] |= BB(ksq);

			nsq = sq + knight_offsets[off];
			if (   nsq <= H8
			    && nsq >= A1
			    && file_diff(sq, nsq) <= 2)
				n_atks[sq] |= BB(nsq);
		}

		for (off = 0; off != 2; ++off) {
			for (c = 0; c != 2; ++c) {
				psq = sq + pawn_offsets[c][off];
				if (   psq <= H8
				    && psq >= A1
				    && file_diff(sq, psq) <= 1)
					p_atks[c][sq] |= BB(psq);
			}
		}

		b_pseudo_atks[sq] = Bmagic(sq, 0ULL);
		r_pseudo_atks[sq] = Rmagic(sq, 0ULL);
		q_pseudo_atks[sq] = Qmagic(sq, 0ULL);
	}
}

void init_intervening_sqs()
{
	int i, j, high, low;
	for (i = 0; i < 64; i++) {
		for (j = 0; j < 64; j++) {
			intervening_sqs[i][j] = 0ULL;
			if (i == j)
				continue;
			high = j;
			if (i > j) {
				high = i;
				low = j;
			}
			else
				low = i;
			if (file_of(high) == file_of(low)) {
				dirn_sqs[i][j] = Rmagic(high, 0ULL) & Rmagic(low, 0ULL);
				for (high -= 8; high != low; high -= 8)
					intervening_sqs[i][j] |= BB(high);
			}
			else if (rank_of(high) == rank_of(low)) {
				dirn_sqs[i][j] = Rmagic(high, 0ULL) & Rmagic(low, 0ULL);
				for (--high; high != low; high--)
					intervening_sqs[i][j] |= BB(high);
			}
			else if (rank_of(high) - rank_of(low) == file_of(high) - file_of(low)) {
				dirn_sqs[i][j] = Bmagic(high, 0ULL) & Bmagic(low, 0ULL);
				for (high -= 9; high != low; high -= 9)
					intervening_sqs[i][j] |= BB(high);
			}
			else if (rank_of(high) - rank_of(low) == file_of(low) - file_of(high)) {
				dirn_sqs[i][j] = Bmagic(high, 0ULL) & Bmagic(low, 0ULL);
				for (high -= 7; high != low; high -= 7)
					intervening_sqs[i][j] |= BB(high);
			}
		}
	}
}

void print_bb(u64 bb)
{
	printf("Bitboard:\n");
	int sq = 0;
	for (; sq != 64; ++sq) {
		if (sq && !(sq & 7))
			printf("\n");

		if ((1ULL << (sq ^ 56)) & bb)
			printf("X  ");
		else
			printf("-  ");
	}
	printf("\n");
}
