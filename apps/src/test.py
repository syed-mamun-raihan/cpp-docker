from bisect import bisect_left, bisect_right

def get_count(wall_positions, start_pos, end_pos): 
    start_wall_pos_idx = bisect_left(wall_positions, start_pos) 
    end_wall_pos_idx = bisect_right(wall_positions, end_pos, lo=start_wall_pos_idx) 
    if start_wall_pos_idx == end_wall_pos_idx: 
        return 0 
    wall_count = end_wall_pos_idx - start_wall_pos_idx 
    start_wall_pos = wall_positions[start_wall_pos_idx] 
    end_wall_pos = wall_positions[end_wall_pos_idx - 1] 
    return end_wall_pos - start_wall_pos - wall_count + 1

def get_counts(inventory, start_idxs, end_idxs): wall_idxs = [i for i, c in enumerate(inventory) if c == '|']