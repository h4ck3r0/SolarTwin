# Stage 1: Build the application
FROM node:26-alpine AS builder
WORKDIR /app

# Copy dependency manifests
COPY package*.json ./
RUN npm ci

# Copy source code and build
COPY . .
ENV NEXT_TELEMETRY_DISABLED=1
RUN npm run build

# Stage 2: Serve the application
FROM node:26-alpine AS runner
WORKDIR /app

ENV NODE_ENV=production
ENV NEXT_TELEMETRY_DISABLED=1

# Copy compiled files and node_modules from builder
COPY --from=builder /app/package*.json ./
COPY --from=builder /app/node_modules ./node_modules
COPY --from=builder /app/.next ./.next
COPY --from=builder /app/public ./public

# Expose Next.js port and start server
EXPOSE 3000
ENV PORT=3000
ENV HOSTNAME="0.0.0.0"

CMD ["npx", "next", "start"]
