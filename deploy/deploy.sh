#! /usr/bin/env bash

set -xeuo pipefail

ENVIRONMENT=$1
TAG=$2
MESSAGE=${3:-"Deployed by $(whoami)"}
IMAGE="gcr.io/exponentjs/snack:$TAG"

gcloud container images describe "$IMAGE"

cd "deploy/${ENVIRONMENT}"

kustomize edit set image "$IMAGE"
kustomize edit add annotation kubernetes.io/change-cause:"$(date): $MESSAGE"

kustomize build | kubectl apply --filename - --validate

kubectl --namespace "$ENVIRONMENT" rollout status deploy snack
