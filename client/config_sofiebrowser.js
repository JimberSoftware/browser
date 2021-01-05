let config = {
    BROKER_HOST: `${window.location.host}`,
    BROKER_HOST_WS: `wss://${window.location.host}/ws`,
    FORCE_MODE: "video",
    ENVIRONMENT: 'sofie',
    CI_COMMIT_HASH: "{CI_COMMIT_HASH}",
    CI_COMMIT_BRANCH: "{CI_COMMIT_BRANCH}",
    CI_COMMIT_TAG: "{CI_COMMIT_TAG}",
    CI_DATE: "{CI_DATE}"
};

export default config;