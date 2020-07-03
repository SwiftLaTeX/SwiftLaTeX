import { Context } from 'koa';
import compose from 'koa-compose';
import Router from 'koa-router';
import { S3 } from 'aws-sdk';
import * as path from 'path';

const AWS_ID = 'Q3AM3UQ867SPQQA43P2F';
const AWS_KEY = 'zuf+tfteSlswRu7BJ86wekitnifILbZam1KYY3TG';
const AWS_ENDPOINT = 'https://play.min.io:9000';
const AWS_BUCKET = 'swiftlatex3';
const s3 = new S3({
    accessKeyId: AWS_ID,
    secretAccessKey: AWS_KEY,
    endpoint: AWS_ENDPOINT,
    s3ForcePathStyle: true,
    signatureVersion: 'v4',
});

const BUCKET_POLICY = {
    Version: '2012-10-17',
    Statement: [
        {
            Action: ['s3:GetObject'],
            Effect: 'Allow',
            Principal: {
                AWS: ['*'],
            },
            Resource: [`arn:aws:s3:::${AWS_BUCKET}/*`],
            Sid: '',
        },
    ],
};

const checkBucketExists = async (bucket: string) => {
    const options = {
        Bucket: bucket,
    };
    try {
        await s3.headBucket(options).promise();
        return true;
    } catch (error) {
        if (error.statusCode === 404) {
            return false;
        }
        throw error;
    }
};

const ensureBucketExist = async () => {
    if (await checkBucketExists(AWS_BUCKET)) {
        return;
    }

    const bucketParams = {
        Bucket: AWS_BUCKET,
        ACL: 'public-read',
    };
    await s3.createBucket(bucketParams).promise();

    const policyParams = {
        Bucket: AWS_BUCKET,
        Policy: JSON.stringify(BUCKET_POLICY),
    };
    await s3.putBucketPolicy(policyParams).promise();
};

const upload = async (ctx: Context) => {
    ctx.type = 'json';
    if (!ctx.request.query.uri) {
        ctx.body = {
            result: 'failed',
        };
        return;
    }

    const fid = path.normalize(ctx.request.query.uri);
    try {
        await ensureBucketExist();
        const params = {
            Bucket: AWS_BUCKET,
            Fields: {
                key: fid,
                ContentType: 'application/octet-stream',
            },
            Condition: [['content-length-range', 0, 2000000]],
        };
        const presigned_url = await new Promise((resolve, reject) => {
            s3.createPresignedPost(params, function (err, data) {
                if (err) {
                    reject(err);
                } else {
                    resolve(data);
                }
            });
        });
        ctx.body = {
            result: 'ok',
            data: presigned_url,
        };
    } catch (e) {
        ctx.body = {
            result: 'failed',
        };
    }
};

const list = async (ctx: Context) => {
    ctx.type = 'json';
    if (!ctx.request.query.uri) {
        ctx.body = {
            result: 'failed',
        };
        return;
    }

    const fid = path.normalize(ctx.request.query.uri);
    try {
        await ensureBucketExist();
        const params = {
            Bucket: AWS_BUCKET,
            Prefix: fid,
        };
        const result = await s3.listObjects(params).promise();
        ctx.body = {
            result: 'ok',
            data: result,
        };
    } catch (e) {
        ctx.body = {
            result: 'failed',
        };
    }
};

const get = async (ctx: Context) => {
    const uri = ctx.request.query.uri;
    if (!uri) {
        ctx.status = 404;
        return;
    }
    ctx.redirect(`${AWS_ENDPOINT}/${AWS_BUCKET}/${uri}`);
};

export default function MinioBackend() {
    const router = new Router();
    router.get('/minio/upload', upload);
    router.get('/minio/list', list);
    router.get('/minio/get', get);
    return compose([router.routes(), router.allowedMethods()]);
}
