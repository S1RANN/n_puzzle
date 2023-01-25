use n_puzzle::{Node, a_star_search};
use std::time::Instant;
fn main() {
    let src = [[12, 15, 6, 10], [4, 9, 5, 8], [14, 13, 0, 2], [1, 7, 11, 3]];
    // let src = [[6,5,2,4],[9,3,1,7],[13,12,15,10],[14,11,8,0]];
    let des = [[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 0]];
    
    let src = Node::from(src);
    let des = Node::from(des);

    let begin = Instant::now();
    let result = a_star_search(&src, &des);
    let elapsed = begin.elapsed();

    for (i, node) in result.iter().enumerate(){
        node.print();
        if i < result.len() - 1{
            println!("          |");
            println!("          ↓");
        }
    }
    println!("moves: {}", result.len() - 1);
    println!("execution time: {}s", elapsed.as_secs_f64());
}