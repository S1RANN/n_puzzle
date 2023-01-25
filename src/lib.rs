use std::clone::Clone;
use std::cmp::Reverse;
use std::collections::{BinaryHeap, HashSet};
use std::hash::Hash;
use std::rc::Rc;

const WIDTH: usize = 4;
pub struct Node {
    chessboard: [[i32; WIDTH]; WIDTH],
    g: i32,
    f: i32,
    p: Option<Rc<Node>>,
}
pub enum Direction {
    Up,
    Down,
    Left,
    Right,
}

impl Clone for Node {
    fn clone(&self) -> Self {
        Self {
            chessboard: self.chessboard.clone(),
            g: self.g.clone(),
            f: self.f.clone(),
            p: self.p.clone(),
        }
    }
}

impl PartialEq for Node {
    fn eq(&self, other: &Self) -> bool {
        self.chessboard == other.chessboard
    }
}

impl Eq for Node {}

impl PartialOrd for Node {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        Some(self.cmp(other))
    }
}

impl Ord for Node {
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        self.f.cmp(&other.f)
    }
}

impl Hash for Node {
    fn hash<H: std::hash::Hasher>(&self, state: &mut H) {
        self.chessboard.hash(state);
    }
}

impl Node {
    pub fn find_location(&self, n: i32) -> (usize, usize) {
        for (i, row) in self.chessboard.iter().enumerate() {
            for (j, e) in row.iter().enumerate() {
                if *e == n {
                    return (i, j);
                }
            }
        }
        (0, 0)
    }

    pub fn from(chessboard: [[i32; WIDTH]; WIDTH]) -> Self {
        Node {
            chessboard,
            g: 0,
            f: 0,
            p: None,
        }
    }

    pub fn get_manhattan_distance(&self, node: &Self) -> i32 {
        let mut distance = 0;

        for (i, row) in self.chessboard.iter().enumerate() {
            for (j, e) in row.iter().enumerate() {
                if *e != 0 {
                    let (x, y) = node.find_location(*e);
                    distance += x.abs_diff(i) + y.abs_diff(j);
                }
            }
        }

        distance as i32
    }

    pub fn step(&self, direction: &Direction) -> Result<Node, &'static str> {
        let (zero_x, zero_y) = self.find_location(0);

        let (stepped_x, stepped_y) = match direction {
            Direction::Up => (zero_x.checked_add_signed(-1), Some(zero_y)),
            Direction::Down => (zero_x.checked_add(1), Some(zero_y)),
            Direction::Left => (Some(zero_x), zero_y.checked_add_signed(-1)),
            Direction::Right => (Some(zero_x), zero_y.checked_add(1)),
        };

        match (stepped_x, stepped_y) {
            (Some(stepped_x), Some(stepped_y)) if stepped_x < WIDTH && stepped_y < WIDTH => {
                let mut stepped_chessboard = self.chessboard;

                stepped_chessboard[zero_x][zero_y] = stepped_chessboard[stepped_x][stepped_y];
                stepped_chessboard[stepped_x][stepped_y] = 0;

                Ok(Node::from(stepped_chessboard))
            }
            _ => Err("Out of bound."),
        }
    }

    pub fn print(&self) {
        println!();
        for i in self.chessboard.iter() {
            print!("  [ ");
            for j in i.iter() {
                if *j > 9 {
                    print!("{j} ");
                } else {
                    print!(" {j} ");
                }
            }
            println!("]");
        }
        println!();
    }
}

pub fn a_star_search(src: &Node, des: &Node) -> Vec<Node> {
    let mut open_list = BinaryHeap::new();
    let mut closed_list = HashSet::new();

    open_list.push(Reverse(src.clone()));

    let mut found = loop {
        let picked = match open_list.pop() {
            Some(picked) => Rc::new(picked.0),
            None => return vec![],
        };
        if *picked == *des {
            break picked;
        }
        closed_list.insert(Rc::clone(&picked));

        let directions = [
            Direction::Up,
            Direction::Down,
            Direction::Left,
            Direction::Right,
        ];

        let mut open_list_v = open_list.into_vec();
        for direction in directions.iter() {
            let mut stepped = match picked.step(direction) {
                Ok(stepped) => stepped,
                Err(_) => continue,
            };
            if closed_list.contains(&stepped) {
                continue;
            }
            if let Some(previous) = open_list_v.iter_mut().find(|node| stepped == node.0) {
                let tmp_g = picked.g + 1;
                let tmp_f = tmp_g + stepped.get_manhattan_distance(des);
                if tmp_f < previous.0.f {
                    previous.0.g = tmp_g;
                    previous.0.f = tmp_f;
                    previous.0.p = Some(Rc::clone(&picked));
                }
            } else {
                stepped.g = picked.g + 1;
                stepped.f = stepped.g + stepped.get_manhattan_distance(des);
                stepped.p = Some(Rc::clone(&picked));
                open_list_v.push(Reverse(stepped));
            }
        }
        open_list = BinaryHeap::from(open_list_v);
    };

    let mut path = vec![];
    path.push((*found).clone());
    while let Some(parent) = &found.p {
        path.push((**parent).clone());
        found = Rc::clone(parent);
    }

    path.reverse();
    path
}
