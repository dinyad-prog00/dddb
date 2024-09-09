const uriParser = (uri: string) => {
    const parsedUrl = new URL(uri);
    if (parsedUrl.protocol !== 'dddb:') {
        throw new Error('Invalid URI scheme. The protocol should be "dddb"');
    }
    let port
    try {
        port = parseInt(parsedUrl.port)
    } catch (e) {
        throw new Error('Invalid port');
    }

    let dbname;
    if (parsedUrl.pathname.length === 0 || parsedUrl.pathname === "/") {
        dbname = "test"
    } else {
        dbname = parsedUrl.pathname.substring(1)
    }
    return { host: parsedUrl.hostname, port: port, username: parsedUrl.username, password: parsedUrl.password, dbname };
}

export default uriParser