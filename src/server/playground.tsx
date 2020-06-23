
import { Context } from 'koa';
import compose from 'koa-compose';
import Router from 'koa-router';
import { S3 } from 'aws-sdk';
import * as path from 'path';

const AWS_ID = process.env.S3ID;
const AWS_KEY = process.env.S3KEY;
const AWS_ENDPOINT = process.env.S3ENDPOINT || 'https://s3.swiftlatex.com';
const AWS_BUCKET = process.env.S3BUCKET || 'swiftlatex2';

const upload = async (ctx: Context) => {
  ctx.type = 'json';
  if (!ctx.request.query.uri) {
    ctx.body = {
      result: 'failed',
    };
    return;
  }

  const s3 = new S3({
    accessKeyId: AWS_ID,
    secretAccessKey: AWS_KEY,
    endpoint: AWS_ENDPOINT,
    s3ForcePathStyle: true,
    signatureVersion: 'v4',
  });

  let fid = path.normalize(ctx.request.query.uri);
  try {
    const params = {
      Bucket: AWS_BUCKET,
      Fields: {
        key: fid,
        ContentType: 'application/octet-stream',
      },
      Condition: [['content-length-range', 0, 2000000]],
    };
    const presigned_url = await new Promise((resolve, reject) => {
      s3.createPresignedPost(params, function(err, data) {
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
  const s3 = new S3({
    accessKeyId: AWS_ID,
    secretAccessKey: AWS_KEY,
    endpoint: AWS_ENDPOINT,
    s3ForcePathStyle: true,
    signatureVersion: 'v4',
  });
  let fid = path.normalize(ctx.request.query.uri);
  try {
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
}


export default function playground() {
  const router = new Router();
  router.get('/upload', upload);
  router.get('/list', list);
  return compose([router.routes(), router.allowedMethods()]);
}
