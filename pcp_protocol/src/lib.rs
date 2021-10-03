extern crate proc_macro;
extern crate syn;
#[macro_use]
extern crate quote;

use proc_macro::TokenStream;
use syn::parse_macro_input;
use proc_macro::TokenTree::Ident;
use std::str::FromStr;

#[proc_macro_derive(pcp_packet, attributes(pcp_opr_id))]
pub fn explain_pcp_packet(input: TokenStream) -> TokenStream {
    let ast: syn::DeriveInput = syn::parse(input).unwrap();

    let struct_name = &ast.ident;
    let mut pcp_opr_id: u16 = 0;
    for i in ast.attrs.iter() {
        let key = format!("{}", i.path.segments[0].ident);
        if key == "pcp_opr_id" {
            let val = format!("{}", i.tokens).replace("(", "").replace(")", "").replace(" ", "");
            if let Ok(val) = u16::from_str(val.as_str()) {
                pcp_opr_id = val;
            }
        }
    }

    if pcp_opr_id == 0 {
        panic!("Unsupport usage of pcp_packet");
    }

    TokenStream::from(quote! {
        impl GenericPcpPacket for #struct_name {
            fn pcp_opr_id() -> u16 {
                #pcp_opr_id
            }

            fn gen_packet(&self) -> PcpPacket {
                let test = String::from("");
                PcpPacket {
                    opr_id: #struct_name::pcp_opr_id(),
                    data_size: test.len() as u16,
                    json_data: test,
                }
            }
        }
    })
}