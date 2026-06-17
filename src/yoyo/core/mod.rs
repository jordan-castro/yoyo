use pixelscript::{own_string, pxs_addfunc, pxs_addmod, pxs_arenaput, pxs_freearena, pxs_getstring, pxs_listget, pxs_listlen, pxs_newarena, pxs_newmod, pxs_newnull, pxs_tostring, shared::{utils::CStringSafe, var::pxs_VarT}};

/// Generate the Yoyo module
pub fn gen_yoyo_module() {
    let mut cstring = CStringSafe::new();
    let yoyo_module = pxs_newmod(cstring.new_string("yoyo"));
    pxs_addfunc(yoyo_module, cstring.new_string("print"), yoyo_print);
    pxs_addmod(yoyo_module);
}

/// `yoyo.print`
unsafe extern "C" fn yoyo_print(args: pxs_VarT) -> pxs_VarT {
    let mut message = String::new();
    let argc = pxs_listlen(args);
    let arena = pxs_newarena();
    let runtime = pxs_listget(args, 0);
    for i in 1..argc {
        let variable = pxs_listget(args, i);
        let to_string = pxs_arenaput(arena, pxs_tostring(runtime, variable));
        let string = own_string!(pxs_getstring(to_string));
        message.push_str(&string);
        if i != argc - 1 {
            message.push(' ');
        }
    }
    pxs_freearena(arena);

    println!("{message}");
    pxs_newnull()
}