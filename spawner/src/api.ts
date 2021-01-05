import express from 'express'
import Docker from 'dockerode'
import {config} from "./config";

const port: number = config.port
const docker: Docker = new Docker({socketPath: '/var/run/docker.sock'});
let webcamCounter: number = 2; // We start at /dev/video2
const maxWebcams = 7;  // The max amount of installed webcams on the server

export const startApi = async () => {
    const app = express();
    app.use(express.json());
    app.post('/container', async (req,res) => {
        const userId = req.body.userId;

        if(!userId) {
            res.json({
                error: 'No userId given.'
            })
            return;
        }

        if(!await isContainerRunning(userId)) {
            docker.createContainer({
                Image: config.dockerImage,
                Tty: false,
                name: userId,
                HostConfig: {
                    AutoRemove: config.autoRemove,
                    NetworkMode: config.dockerNetwork,
                    Devices: generateDevices(),
                    Binds: await generateBinds(userId)
                },
                Labels: {jimber: "browser"},
                // Volumes: await generateVolumes(userId),
                Env: generateEnvironment(userId),
            }).then((container) => {
                container.start();
            })
        }

        res.send({
            'downloadsEnabled': config.downloadsEnabled,
            'userId': userId
        })
    })

    app.listen(port)
}

const generateVolumes = async (userId: string): Promise<any> => {
    const volumes: any = {};
    if(!userId) {
        return {};
    }

    if(config.persistentClientStorage) {
        const volumeName = `browser_storage${userId}`;
        const test = await docker.createVolume({
            name: volumeName,
            labels: {'jimber': 'volume'}
        })
        console.log(test)
        // volumes[volumeName] = {'bind': "/root/.local/share/browser/QtWebEngine/Default", 'mode': 'rw'}
        volumes["/root/.local/share/browser/QtWebEngine/Default"] = {'name': volumeName, 'mode': 'rw'}
        // volumes[`${volumeName}:/root/.local/share/browser/QtWebEngine/Default`]
    }

    return volumes;
}

const generateBinds = async (userId: string): Promise<any> => {
    const binds: any = [];
    if(!userId) {
        return [];
    }

    if(config.persistentClientStorage) {
        const volumeName = `browser_storage${userId}`;
        await docker.createVolume({
            name: volumeName,
            labels: {'jimber': 'volume'}
        })
        // volumes[volumeName] = {'bind': "/root/.local/share/browser/QtWebEngine/Default", 'mode': 'rw'}
        // binds.push["/root/.local/share/browser/QtWebEngine/Default"] = {'name': volumeName, 'mode': 'rw'}
        binds.push(`${volumeName}:/root/.local/share/browser/QtWebEngine/Default`)
    }

    if(config.downloadsEnabled) {
        binds.push(`/opt/jimber/downloads/${userId}:/jimber_downloads:rw`);
    }

    return binds;
}

const generateEnvironment = (userId: string) => {
    const environment: any = [];
    environment.push(`WEBCAM_ENABLED=${config.webcamEnabled}`);
    environment.push(`USER_ID=${userId}`);
    return environment;
}

const generateDevices = () => {
    const devices: any = []
    if(config.webcamEnabled) {
        const webcamId = createWebcam();
        devices.push(`/dev/video${webcamId}:/dev/video0`);
    }
}

const createWebcam = () => { // Testcode from @azertyalex
    const webcamId = webcamCounter;
    webcamCounter += 1;
    if(webcamCounter > maxWebcams) {
        webcamCounter = 2;
    }
    return webcamId;
}

const isContainerRunning = async (userId: string): Promise<boolean> => {
    const containers = await docker.listContainers({filters: { "name": [userId] }})
    return containers.length > 0;
}