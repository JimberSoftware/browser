import hashCode from '../Extensions.js'

export class CookieJar {
    constructor() {
        this.cookies = {};
        this._load();
    }
    _save() {

        localStorage.setItem("cookies", JSON.stringify(this.cookies));
    }
    _load() {
        if (localStorage.getItem("cookies") != null) {
            var cookiesToAdd = JSON.parse(localStorage.getItem("cookies"));
            for (const cookieObj of Object.values(cookiesToAdd)) {
                this.addCookie(new Cookie(cookieObj))
            }
        }
    }

    addCookie(cookie) {
        this.cookies[cookie.hash] = cookie;
        //  console.log("merged cookie", this.cookies[cookie.hash])
        this._save();
    }
    delCookie(cookie) {
        delete this.cookies[cookie.hash]
        this._save();
    }
    all() {

        var cookiesInString = [];
        for (let cookie of Object.values(this.cookies)) {
            cookiesInString.push(cookie.toString());
        }

        return cookiesInString;
    }
}

export class Cookie {

    constructor({ key, value, domain, path, expires, HttpOnly, secure }) {
        this.key = key;
        this.value = value;
        this.domain = domain;
        this.path = path;
        this.expires = expires;
        this.HttpOnly = HttpOnly;
        this.secure = secure;
        this.hash = `${this.domain}${this.path}${this.key}`.hashCode();

    }

    toString() {

        return `${this.key}=${this.value}; ${this.expires}; domain=${this.domain}; ${this.secure ? 'secure;' : ''} ${this.HttpOnly ? 'HttpOnly;' : ''} path=${this.path}`
    }

    static fromCookieString(cookieString) {
        const parseCookieString = cookieString => {
            const splitted = cookieString.split(/; */);

            // get key value from first element in cookie string
            const [key, value] = splitted.shift().split(/=(.+)/);

            return splitted.reduce(
                (object, part) => {
                    const parts = part.split(/=(.+)/);
                    object[parts[0]] = parts[1] ? parts[1] : true;
                    return object;
                },
                { key, value }
            );
        };
        const parsed = parseCookieString(cookieString);

        return new Cookie(parsed);



    }
}

