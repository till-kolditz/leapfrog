use leapfrog::{LeapFrogJoin, LinearIterator};

fn main() {
    let tab1 = vec![0, 1, 3, 4, 5, 6, 7, 8, 9, 11];
    let tab2 = vec![0, 2, 6, 7, 8, 9, 11];
    let tab3 = vec![2, 4, 5, 8, 10];

    // Demonstrate LinearIterator on tab1
    {
        let mut iter = LinearIterator::new(&tab1);
        println!("tab1:");
        while !iter.at_end() {
            println!("\t{}", iter.key());
            iter.next();
        }
    }

    // Demonstrate LinearIterator on tab2
    {
        let mut iter = LinearIterator::new(&tab2);
        println!("tab2:");
        while !iter.at_end() {
            println!("\t{}", iter.key());
            iter.next();
        }
    }

    // Demonstrate LinearIterator on tab3
    {
        let mut iter = LinearIterator::new(&tab3);
        println!("tab3:");
        while !iter.at_end() {
            println!("\t{}", iter.key());
            iter.next();
        }
    }

    // Demonstrate LeapFrogJoin
    {
        let mut join = LeapFrogJoin::new(vec![&tab1, &tab2, &tab3]);
        println!("join(tab1, tab2, tab3):");
        while !join.at_end() {
            println!("\t{}", join.key());
            join.next();
        }
    }
}
