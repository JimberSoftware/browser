import {readdirSync} from "fs";
import path from 'path';

export const loadModules = () => {
    const directories = getDirectories(path.resolve(__dirname, 'modules'));
    for (let directory of directories) {
        console.log(`Trying to initialize ${directory} module`);
        import(`./modules/${directory}/index`).then(module => {
            module.instance.init();
            console.log(`Initialized ${directory} module`);
        }).catch(error => {
            console.error(`Failed initializing ${directory} module`, error);
        })
    }
}

const getDirectories = (source: string) => {
    return readdirSync(source, { withFileTypes: true })
        .filter(dirent => dirent.isDirectory())
        .map(dirent => dirent.name)
}
