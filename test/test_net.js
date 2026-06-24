import { get } from 'yoyo.net';
import * as yoyo from 'yoyo';

const _make_request = (url, type=null, headers=null, body=null) => {
    // TODO
};

let response = get("https://jsonplaceholder.typicode.com/todos/1");
yoyo.println(response.status);
yoyo.println(response.version);
yoyo.println(response.text);
yoyo.println(response.bytes);