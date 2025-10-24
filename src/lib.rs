use std::cmp::Ordering;

/// LinearIterator provides iteration over a vector with specific operations
/// required by the leapfrog join algorithm.
///
/// The paper requires the linear iterator to have certain time complexity
/// bounds on the interface methods, which are not really satisfied here, but
/// then, this is just a toy implementation using vectors.
///
/// As a result, key() and at_end() have the desired time complexity O(1), while,
/// next() and seek() have time complexity O(N) instead of O(log N).
#[derive(Clone)]
pub struct LinearIterator<'a, T> {
    source: &'a [T],
    pos: usize,
}

impl<'a, T> LinearIterator<'a, T> {
    pub fn new(source: &'a [T]) -> Self {
        Self { source, pos: 0 }
    }

    pub fn at_end(&self) -> bool {
        self.pos >= self.source.len()
    }
}

impl<'a, T> LinearIterator<'a, T>
where
    T: Ord + Copy,
{
    pub fn key(&self) -> T {
        assert!(!self.at_end(), "Iterator is at end");
        self.source[self.pos]
    }

    pub fn next(&mut self) {
        assert!(!self.at_end(), "Iterator is at end");
        self.pos += 1;
    }

    pub fn seek(&mut self, seek_key: T) {
        assert!(!self.at_end(), "Iterator is at end");
        assert!(
            seek_key >= self.source[self.pos],
            "Seek key must be >= current key"
        );
        while !self.at_end() && self.source[self.pos] < seek_key {
            self.next();
        }
    }
}

impl<'a, T: Ord + Copy> PartialEq for LinearIterator<'a, T> {
    fn eq(&self, other: &Self) -> bool {
        if self.at_end() && other.at_end() {
            true
        } else if self.at_end() || other.at_end() {
            false
        } else {
            self.key() == other.key()
        }
    }
}

impl<'a, T: Ord + Copy> Eq for LinearIterator<'a, T> {}

impl<'a, T: Ord + Copy> PartialOrd for LinearIterator<'a, T> {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

impl<'a, T: Ord + Copy> Ord for LinearIterator<'a, T> {
    fn cmp(&self, other: &Self) -> Ordering {
        if self.at_end() && other.at_end() {
            Ordering::Equal
        } else if self.at_end() {
            Ordering::Greater
        } else if other.at_end() {
            Ordering::Less
        } else {
            self.key().cmp(&other.key())
        }
    }
}

/// LeapFrogJoin implements the leapfrog join algorithm for finding
/// common elements across multiple sorted vectors.
pub struct LeapFrogJoin<'a, T> {
    iters: Vec<LinearIterator<'a, T>>,
    iters_indices: Vec<usize>,
    at_end: bool,
    pos: usize,
}

impl<'a, T> LeapFrogJoin<'a, T>
where
    T: Ord + Copy,
{
    pub fn new(sources: Vec<&'a [T]>) -> Self {
        let iters: Vec<LinearIterator<'a, T>> =
            sources.iter().map(|&s| LinearIterator::new(s)).collect();

        let mut at_end = true;
        for iter in &iters {
            at_end &= iter.at_end();
        }

        let mut iters_indices: Vec<usize> = (0..iters.len()).collect();

        if !at_end {
            // Sort iterators by their current key
            iters_indices.sort_by(|&a, &b| iters[a].cmp(&iters[b]));

            let mut join = Self {
                iters,
                iters_indices,
                at_end,
                pos: 0,
            };

            join.search();
            join
        } else {
            Self {
                iters,
                iters_indices,
                at_end,
                pos: 0,
            }
        }
    }

    pub fn key(&self) -> T {
        assert!(!self.at_end, "Join is at end");
        self.iters[self.iters_indices[0]].key()
    }

    pub fn next(&mut self) {
        assert!(!self.at_end, "Join is at end");
        let cur_idx = self.iters_indices[self.pos];
        self.iters[cur_idx].next();

        if self.iters[cur_idx].at_end() {
            self.at_end = true;
        } else {
            self.pos = self.next_pos();
            self.search();
        }
    }

    pub fn seek(&mut self, seek_key: T) {
        assert!(!self.at_end, "Join is at end");
        let cur_idx = self.iters_indices[self.pos];
        self.iters[cur_idx].seek(seek_key);

        if self.iters[cur_idx].at_end() {
            self.at_end = true;
        } else {
            self.pos = self.next_pos();
            self.search();
        }
    }

    pub fn at_end(&self) -> bool {
        self.at_end
    }

    fn search(&mut self) {
        assert!(!self.at_end, "Join is at end");
        let prev_idx = self.iters_indices[self.prev_pos()];
        let mut max_key = self.iters[prev_idx].key();

        loop {
            let cur_idx = self.iters_indices[self.pos];
            let cur_key = self.iters[cur_idx].key();

            if cur_key == max_key {
                break;
            } else {
                self.iters[cur_idx].seek(max_key);
                if self.iters[cur_idx].at_end() {
                    self.at_end = true;
                    break;
                } else {
                    max_key = self.iters[cur_idx].key();
                    self.pos = (self.pos + 1) % self.iters.len();
                }
            }
        }
    }

    fn prev_pos(&self) -> usize {
        (self.pos + self.iters.len() - 1) % self.iters.len()
    }

    fn next_pos(&self) -> usize {
        (self.pos + 1) % self.iters.len()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn tab0() -> Vec<i32> {
        vec![]
    }

    fn tab1() -> Vec<i32> {
        vec![0, 1, 3, 4, 5, 6, 7, 8, 9, 11]
    }

    fn tab2() -> Vec<i32> {
        vec![0, 2, 6, 7, 8, 9, 11]
    }

    fn tab3() -> Vec<i32> {
        vec![2, 4, 5, 8, 10]
    }

    #[test]
    fn test_linear_iterator_init0() {
        let tab0 = tab0();
        let iter = LinearIterator::new(&tab0);
        assert!(iter.at_end());
    }

    #[test]
    fn test_linear_iterator_init1() {
        let tab1 = tab1();
        let iter = LinearIterator::new(&tab1);
        assert!(!iter.at_end());
        assert_eq!(iter.key(), 0);
    }

    #[test]
    fn test_linear_iterator_init2() {
        let tab2 = tab2();
        let iter = LinearIterator::new(&tab2);
        assert!(!iter.at_end());
        assert_eq!(iter.key(), 0);
    }

    #[test]
    fn test_linear_iterator_init3() {
        let tab3 = tab3();
        let iter = LinearIterator::new(&tab3);
        assert!(!iter.at_end());
        assert_eq!(iter.key(), 2);
    }

    #[test]
    fn test_linear_iterator_next1() {
        let tab1 = tab1();
        let mut iter = LinearIterator::new(&tab1);
        iter.next();
        assert!(!iter.at_end());
        assert_eq!(iter.key(), 1);
    }

    #[test]
    fn test_linear_iterator_next2() {
        let tab2 = tab2();
        let mut iter = LinearIterator::new(&tab2);
        iter.next();
        assert!(!iter.at_end());
        assert_eq!(iter.key(), 2);
    }

    #[test]
    fn test_linear_iterator_next3() {
        let tab3 = tab3();
        let mut iter = LinearIterator::new(&tab3);
        iter.next();
        assert!(!iter.at_end());
        assert_eq!(iter.key(), 4);
    }

    #[test]
    fn test_linear_iterator_seek1() {
        let tab1 = tab1();
        let mut iter = LinearIterator::new(&tab1);
        iter.seek(2);
        assert!(!iter.at_end());
        assert_eq!(iter.key(), 3);
        iter.seek(3);
        assert!(!iter.at_end());
        assert_eq!(iter.key(), 3);
        iter.seek(8);
        assert!(!iter.at_end());
        assert_eq!(iter.key(), 8);
        iter.seek(10);
        assert!(!iter.at_end());
        assert_eq!(iter.key(), 11);
        iter.seek(12);
        assert!(iter.at_end());
    }

    #[test]
    fn test_linear_iterator_seek2() {
        let tab2 = tab2();
        let mut iter = LinearIterator::new(&tab2);
        iter.seek(1);
        assert!(!iter.at_end());
        assert_eq!(iter.key(), 2);
        iter.seek(4);
        assert!(!iter.at_end());
        assert_eq!(iter.key(), 6);
        iter.seek(8);
        assert!(!iter.at_end());
        assert_eq!(iter.key(), 8);
        iter.seek(10);
        assert!(!iter.at_end());
        assert_eq!(iter.key(), 11);
        iter.seek(12);
        assert!(iter.at_end());
    }

    #[test]
    #[should_panic(expected = "Iterator is at end")]
    fn test_linear_iterator_next0() {
        let tab0 = tab0();
        let mut iter = LinearIterator::new(&tab0);
        iter.next();
    }

    #[test]
    #[should_panic(expected = "Iterator is at end")]
    fn test_linear_iterator_seek0() {
        let tab0 = tab0();
        let mut iter = LinearIterator::new(&tab0);
        iter.seek(3);
    }

    #[test]
    fn test_leapfrog_join_init2() {
        let tab1 = tab1();
        let tab2 = tab2();
        let mut join = LeapFrogJoin::new(vec![&tab1, &tab2]);
        assert!(!join.at_end());
        for expected in [0, 6, 7, 8, 9, 11] {
            assert_eq!(join.key(), expected);
            join.next();
        }
        assert!(join.at_end());
    }

    #[test]
    fn test_leapfrog_join_init3() {
        let tab1 = tab1();
        let tab2 = tab2();
        let tab3 = tab3();
        let mut join = LeapFrogJoin::new(vec![&tab1, &tab2, &tab3]);
        assert!(!join.at_end());
        assert_eq!(join.key(), 8);
        join.next();
        assert!(join.at_end());
    }
}
