#[macro_use]
extern crate pcp_protocol;

mod protocol;

use crate::protocol::{LoginReq, PcpPacket};


fn main() {
	let a = protocol::LoginReq {
		username: "".to_string(),
		password: "".to_string()
	};
	let b = String::from("abc");

	// protocol::LoginReq::like();
	// a.like();
	println!("Hello, world!");
}
