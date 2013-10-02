/*
  Author: Harshavardhana <fharshav@redhat.com>
  Copyright (c) 2013 Red Hat, Inc. <http://www.redhat.com>

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

typedef unsigned char uchar_t;

struct s3_conf {
        char *account_id; /* AWS Account ID */
        char *awskey_id; /* AWS Key ID */
        char *awskey; /* AWS Key */
        char *s3host; /* AWS S3 Hostname eg: "s3.amazonaws.com" */
        char *bucket_name; /* S3 bucket name */

        /* Not supported yet */
        int32_t use_rrs; /* Use reduced redundancy storage */
        char *mime_type; /* Mimetype */
        char *acls;
};
