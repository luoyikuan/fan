import { Base64 } from 'js-base64'

// 读取配置:JSON Base64编码
// {
//   "mqtt": {
//     "url": null,
//     "username": null,
//     "password": null
//   },
//   "topic": {
//     "ctrl": 'fan/ctrl/xxx',
//     "data": 'fan/data/xxx'
//   }
// }
export function getConfig() {
    let str = new URLSearchParams(window.location.search).get('config');
    return JSON.parse(Base64.decode(str || ''))
}