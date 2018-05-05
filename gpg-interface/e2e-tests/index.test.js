
const fetch = require('node-fetch')

const url = 'http://127.0.0.1:3000'

describe('Authentication and challenges', () => {
  it('should reject getting messages if no authentication', () => {
    return fetch(url + '/messages')
      .then((res) => {
        expect(res.status).toEqual(403)
      });
  });

  it('should reject getting messages if authentication code wrong', () => {
    return fetch(url + '/messages', {
        method: 'GET',
        headers: { Authentication: 'hello'},
      })
      .then((res) => {
        expect(res.status).toEqual(403)
      });
  });

  it('should be able to request a challenge', () => {
    return fetch(url + '/get_challenge', { 
        method: 'POST',
        body: JSON.stringify({ email: "coolio@cools.com" })
    })
      .then((res) => expect(res.status).toEqual(200))
  });

  it('should not authorize with unsolved challenge, or use it after failure', () => {
    let gotToken = "";
    return fetch(url + '/get_challenge', {
        method: 'POST',
        body: JSON.stringify({ email: "coolio@cools.com" })
      })
      .then((res) => {
        expect(res.status).toEqual(200);
        return res.text()
      })
      .then((token) => {
        gotToken = token;
        const mockSolution = `-----BEGIN PGP SIGNATURE-----

iQFKBAEBCAA0FiEEb91H/xtBKjkEIVh1M4rngnnmoIUFAlrnkUoWHGJ1bGxzaGl0
QGJ1bGxzaGl0LmNvbQAKCRAziueCeeaghZ71CACEYL3CsDdYkcj95f+stGvSgEmw
SjddnpCAIyGDdJUXtHJ9iAClllNS+hRhvxxxXvLFwf/HGS5rUJPFnROlHjixSQ2j
yfqEWHbs7aMlWWjmDh2LAZrPq2eGlEuKsy8wqMDe4vba+hV/rOlPKx6gjRGJ3vHV
H1q+YiI+7+idEQLV/Oc1CF1DRNd3utkpQismgCNNktrf5OKLALupPqWT+iYEaN7H
MUUc7X2GpC1Kie3GX2ASCZO1mHfHMF06pIChRPHxeSCLaSZpvi52KMPbby8gnLKI
9RDjtWUy9SbPwBissAZKuyTG6XhhpXNWExMYHEPU8BhKy8SNF/hd5o6OdXDy
=zhvn
-----END PGP SIGNATURE-----`
        return fetch(url + '/challenge', {
          method: 'POST',
          body: JSON.stringify({
            email: "coolio@cools.com",
            solution: mockSolution,
          })
        })
      })
      .then((res) => expect(res.status).toEqual(403))
      .then((res) => {
        
      });
  });


  it('should give 403 with unrecognized email', () => {
      return fetch(url + '/challenge', {
          method: 'POST',
          body: JSON.stringify({
            email: "fake@fake123.com",
          })
        })
      .then((res) => expect(res.status).toEqual(403))
  });


});
