import { IDeviceModel, DEFAULT_START_OPTIONS } from '@zondax/zemu'

import { resolve } from 'path'

export const APP_SEED = 'equip will roof matter pink blind book anxiety banner elbow sun young'

const APP_PATH_S = resolve('../app/output/app_s.elf')
const APP_PATH_X = resolve('../app/output/app_x.elf')
const APP_PATH_SP = resolve('../app/output/app_s2.elf')
const APP_PATH_ST = resolve('../app/output/app_stax.elf')

export const models: IDeviceModel[] = [
  { name: 'nanos', prefix: 'S', path: APP_PATH_S },
  { name: 'nanox', prefix: 'X', path: APP_PATH_X },
  { name: 'nanosp', prefix: 'SP', path: APP_PATH_SP },
  { name: 'stax', prefix: 'ST', path: APP_PATH_ST },
]

export const PATH = "m/44'/318'/0'/0/0"

export const defaultOptions = {
  ...DEFAULT_START_OPTIONS,
  logging: true,
  custom: `-s "${APP_SEED}"`,
  X11: false,
}

export const MATRIX_TRANSACTIONS = [
  {
    name: 'normalTxn',
    blob: Buffer.from(
      'f8c0871000000000000f850430e2340083033450a14d414e2e32556f7a3867386a61754d61326d746e7778727363686a3271504a724583989680800380808080845c3d93c9f87bf8798080f875e6a04d414e2e6a4c5446686f434a43474368706964553269433151357a436d56464c8398968080e6a04d414e2e66344657484562576b583873536438796a5a6a5948655a576e6164788398968080e6a04d414e2e675141414855655442787667627a6638744667557461764463654a508398968080',
      'hex',
    ),
  },
  {
    name: 'authorizeTxn',
    blob: Buffer.from(
      'f8cd871000000000000e850430e2340083033450a04d414e2e576b62756a7478683759426e6b475638485a767950514b336341507980b8885b7b22456e7472757374416464726573223a224d414e2e3661706346595162595a68774c5a7a33626234546a666b67346d794a222c224973456e7472757374476173223a747275652c22456e73747275737453657454797065223a302c225374617274486569676874223a323232323232322c22456e64486569676874223a323232323232357d5d0380808080845c3d93c9c4c30580c0',
      'hex',
    ),
  },
]
