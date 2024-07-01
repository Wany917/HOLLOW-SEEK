import type { HttpContext } from '@adonisjs/core/http'
import { registerValidator } from '#validators/register'
import { loginValidator } from '#validators/login'
import User from '#models/user'

export default class AuthController {
    static async register({ request, response }: HttpContext) {
      try {
        const payload = await request.validateUsing(registerValidator)
        
        const fullName = `${payload.firstname} ${payload.lastname}`;
        const user = await User.create({
            fullName,
            email: payload.email,
            password: payload.password 
        })

        return response.created(user)
      } catch (error) {
        console.error(error);
        return response.status(400).send(error);
      }
    }

   /**
   * Handle form submission for the login action
   * @returns user as JSON response
   * @error 401 on invalid credentials
   */

   static async login({ request, response }: HttpContext) {
    try {
      const { email, password } = await request.validateUsing(loginValidator)
      const user = await User.verifyCredentials(email, password)
      const token = await User.accessTokens.create(user)
      return response.ok({
        token: token.value!.release(),
        ...user.serialize(),
      })
    } catch (error) {
      return response.status(401).send('Invalid credentials')
    }
  }

  /**
   * Display the authenticated user
   * @returns authenticated user as JSON response
   * @error 401 on unauthorized
   */
  
  static async me({ auth, response }: HttpContext) {
    try {
      const user = await auth.authenticate()
      return response.ok(user)
    } catch (error) {
      return response.status(401).send('Unauthorized')
    }
  }

}
