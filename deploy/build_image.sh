#!/usr/bin/env bash

set -eo pipefail

environment="$1"
tag="$2"

snack_dir="$(pwd)"

if [[ "$snack_dir" != "$EXPO_UNIVERSE_DIR/server/snack" ]]
then
  echo 'not in snack directory'
  exit 1
fi

owner="gcr.io/exponentjs"
image="snack"

ifprod() {
  if [[ "$environment" == "production" ]]
  then
    echo "$1"
  elif [[ "$environment" == "staging" ]]
  then
    echo "$2"
  else
    echo "unknown environment specified"
    exit 1
  fi
}

envtag="$environment-$tag"
buildargs="--build-arg API_SERVER_URL=$(ifprod https://expo.io https://staging.expo.io) $buildargs"
buildargs="--build-arg IMPORT_SERVER_URL=$(ifprod https://snackager.expo.io https://staging.snackager.expo.io) $buildargs"
buildargs="--build-arg DEPLOY_ENVIRONMENT=$(ifprod production staging) $buildargs"
buildargs="--build-arg CDN_URL=$(ifprod https://dejalo84wis46.cloudfront.net https://d30hq726efxt5o.cloudfront.net) $buildargs"
buildargs="--build-arg SNACK_SEGMENT_KEY=$(ifprod Ha0swpI6s2CVEMxK84cEmKmUVmBa1USu dxul6twMnfpyguF8w4W2qUpFnhxEUSV6) $buildargs"
if [[ "$environment" == "staging" ]]
then
    buildargs="--build-arg SNACK_APP_URL=http://s3.us-west-1.amazonaws.com/snack-web-player-staging/36 $buildargs"
fi

if [ ! -z "$tag" ]; then
  buildargs="$buildargs --build-arg APP_VERSION=$envtag"
  tagargs="-t $owner/$image:$envtag"
fi

# shellcheck disable=SC2086
docker build --file deploy/Dockerfile $buildargs $tagargs .

if [ ! -z "$tag" ]; then
  retry5 docker push "$owner/$image:$envtag"
fi
